#pragma once

/* Document Markup Language (DML) v1.0 parser
 * revision 0.05
 */

#include <nall/location.hpp>

namespace nall {

struct DML {
  inline auto title() const -> string { return state.title; }
  inline auto subtitle() const -> string { return state.subtitle; }
  inline auto description() const -> string { return state.description; }
  inline auto content() const -> string { return state.output; }

  auto& setAllowHTML(bool allowHTML) { settings.allowHTML = allowHTML; return *this; }
  auto& setHost(const string& hostname) { settings.host = {hostname, "/"}; return *this; }
  auto& setPath(const string& pathname) { settings.path = pathname; return *this; }
  auto& setReader(const function<string (string)>& reader) { settings.reader = reader; return *this; }
  auto& setSectioned(bool sectioned) { settings.sectioned = sectioned; return *this; }

  auto parse(const string& filedata, const string& pathname) -> string;
  auto parse(const string& filename) -> string;

  auto attribute(const string& name) const -> string;

private:
  struct Settings {
    bool allowHTML = true;
    string host = "localhost/";
    string path;
    function<string (string)> reader;
    bool sectioned = true;
  } settings;

  struct State {
    string title;
    string subtitle;
    string description;
    string output;
    uint sections = 0;
  } state;

  struct Attribute {
    string name;
    string value;
  };
  vector<Attribute> attributes;

  auto parseDocument(const string& filedata, const string& pathname, uint depth) -> bool;
  auto parseBlock(string& block, const string& pathname, uint depth) -> bool;
  auto count(const string& text, char value) -> uint;

  auto escape(const string& text) -> string;
  auto markup(const string& text) -> string;
};

inline auto DML::attribute(const string& name) const -> string {
  for(auto& attribute : attributes) {
    if(attribute.name == name) return attribute.value;
  }
  return {};
}

inline auto DML::parse(const string& filedata, const string& pathname) -> string {
  state = {};
  settings.path = pathname;
  parseDocument(filedata, settings.path, 0);
  return state.output;
}

inline auto DML::parse(const string& filename) -> string {
  state = {};
  if(!settings.path) settings.path = Location::path(filename);
  string document = settings.reader ? settings.reader(filename) : string::read(filename);
  parseDocument(document, settings.path, 0);
  return state.output;
}

inline auto DML::parseDocument(const string& filedata, const string& pathname, uint depth) -> bool {
  if(depth >= 100) return false;  //attempt to prevent infinite recursion with reasonable limit

  auto blocks = filedata.split("\n\n");
  for(auto& block : blocks) parseBlock(block, pathname, depth);
  if(settings.sectioned && state.sections && depth == 0) state.output.append("</section>\n");
  return true;
}

inline auto DML::parseBlock(string& block, const string& pathname, uint depth) -> bool {
  if(!block.stripRight()) return true;
  auto lines = block.split("\n");

  //include
  if(block.beginsWith("<include ") && block.endsWith(">")) {
    string filename{pathname, block.trim("<include ", ">", 1L).strip()};
    string document = settings.reader ? settings.reader(filename) : string::read(filename);
    parseDocument(document, Location::path(filename), depth + 1);
  }

  //html
  else if(block.beginsWith("<html>\n") && settings.allowHTML) {
    for(auto n : range(lines.size())) {
      if(n == 0 || !lines[n].beginsWith("  ")) continue;
      state.output.append(lines[n].trimLeft("  ", 1L), "\n");
    }
  }

  //attribute
  else if(block.beginsWith("! ")) {
    for(auto& line : lines) {
      auto parts = line.trimLeft("! ", 1L).split(":", 1L);
      if(parts.size() == 2) attributes.append({parts[0].strip(), parts[1].strip()});
    }
  }

  //description
  else if(block.beginsWith("? ")) {
    while(lines) {
      state.description.append(lines.takeLeft().trimLeft("? ", 1L), " ");
    }
    state.description.strip();
  }

  //section
  else if(block.beginsWith("# ")) {
    if(settings.sectioned) {
      if(state.sections++) state.output.append("</section>");
      state.output.append("<section>");
    }
    auto content = lines.takeLeft().trimLeft("# ", 1L).split("::", 1L).strip();
    auto data = markup(content[0]);
    auto name = escape(content(1, data.hash()));
    state.subtitle = content[0];
    state.output.append("<h2 id=\"", name, "\">", data);
    for(auto& line : lines) {
      if(!line.beginsWith("# ")) continue;
      state.output.append("<span>", line.trimLeft("# ", 1L), "</span>");
    }
    state.output.append("</h2>\n");
  }

  //header
  else if(auto depth = count(block, '=')) {
    auto content = slice(lines.takeLeft(), depth + 1).split("::", 1L).strip();
    auto data = markup(content[0]);
    auto name = escape(content(1, data.hash()));
    if(depth <= 4) {
      state.output.append("<h", depth + 2, " id=\"", name, "\">", data);
      for(auto& line : lines) {
        if(count(line, '=') != depth) continue;
        state.output.append("<span>", slice(line, depth + 1), "</span>");
      }
      state.output.append("</h", depth + 2, ">\n");
    }
  }

  //navigation
  else if(count(block, '-')) {
    state.output.append("<nav>\n");
    uint level = 0;
    for(auto& line : lines) {
      if(auto depth = count(line, '-')) {
        while(level < depth) level++, state.output.append("<ul>\n");
        while(level > depth) level--, state.output.append("</ul>\n");
        auto content = slice(line, depth + 1).split("::", 1L).strip();
        auto data = markup(content[0]);
        auto name = escape(content(1, data.hash()));
        state.output.append("<li><a href=\"#", name, "\">", data, "</a></li>\n");
      }
    }
    while(level--) state.output.append("</ul>\n");
    state.output.append("</nav>\n");
  }

  //list
  else if(count(block, '*')) {
    uint level = 0;
    for(auto& line : lines) {
      if(auto depth = count(line, '*')) {
        while(level < depth) level++, state.output.append("<ul>\n");
        while(level > depth) level--, state.output.append("</ul>\n");
        auto data = markup(slice(line, depth + 1));
        state.output.append("<li>", data, "</li>\n");
      }
    }
    while(level--) state.output.append("</ul>\n");
  }

  //quote
  else if(count(block, '>')) {
    uint level = 0;
    for(auto& line : lines) {
      if(auto depth = count(line, '>')) {
        while(level < depth) level++, state.output.append("<blockquote>\n");
        while(level > depth) level--, state.output.append("</blockquote>\n");
        auto data = markup(slice(line, depth + 1));
        state.output.append(data, "\n");
      }
    }
    while(level--) state.output.append("</blockquote>\n");
  }

  //code
  else if(block.beginsWith("  ")) {
    state.output.append("<pre>");
    for(auto& line : lines) {
      if(!line.beginsWith("  ")) continue;
      state.output.append(escape(line.trimLeft("  ", 1L)), "\n");
    }
    state.output.trimRight("\n", 1L).append("</pre>\n");
  }

  //divider
  else if(block.equals("---")) {
    state.output.append("<hr>\n");
  }

  //paragraph
  else {
    auto content = markup(block);
    if(content.beginsWith("<figure") && content.endsWith("</figure>")) {
      state.output.append(content, "\n");
    } else {
      state.output.append("<p>", content, "</p>\n");
    }
  }

  return true;
}

inline auto DML::count(const string& text, char value) -> uint {
  for(uint n = 0; n < text.size(); n++) {
    if(text[n] != value) {
      if(text[n] == ' ') return n;
      break;
    }
  }
  return 0;
}

inline auto DML::escape(const string& text) -> string {
  string output;
  for(auto c : text) {
    if(c == '&') { output.append("&amp;"); continue; }
    if(c == '<') { output.append("&lt;"); continue; }
    if(c == '>') { output.append("&gt;"); continue; }
    if(c == '"') { output.append("&quot;"); continue; }
    output.append(c);
  }
  return output;
}

inline auto DML::markup(const string& s) -> string {
  string t;

  boolean strong;
  boolean emphasis;
  boolean insertion;
  boolean deletion;
  boolean code;

  maybe<uint> link;
  maybe<uint> image;

  for(uint n = 0; n < s.size();) {
    char a = s[n];
    char b = s[n + 1];

    if(!link && !image) {
      if(a == '*' && b == '*') { t.append(strong.flip() ? "<strong>" : "</strong>"); n += 2; continue; }
      if(a == '/' && b == '/') { t.append(emphasis.flip() ? "<em>" : "</em>"); n += 2; continue; }
      if(a == '_' && b == '_') { t.append(insertion.flip() ? "<ins>" : "</ins>"); n += 2; continue; }
      if(a == '~' && b == '~') { t.append(deletion.flip() ? "<del>" : "</del>"); n += 2; continue; }
      if(a == '|' && b == '|') { t.append(code.flip() ? "<code>" : "</code>"); n += 2; continue; }
      if(a =='\\' && b =='\\') { t.append("<br>"); n += 2; continue; }

      if(a == '[' && b == '[') { n += 2; link = n; continue; }
      if(a == '{' && b == '{') { n += 2; image = n; continue; }
    }

    if(link && !image && a == ']' && b == ']') {
      auto list = slice(s, link(), n - link()).split("::", 1L);
      string uri = list.last();
      string name = list.size() == 2 ? list.first() : list.last().split("//", 1L).last();

      t.append("<a href=\"", escape(uri), "\">", escape(name), "</a>");

      n += 2;
      link = nothing;
      continue;
    }

    if(image && !link && a == '}' && b == '}') {
      auto side = slice(s, image(), n - image()).split("}{", 1L);
      auto list = side(0).split("::", 1L);
      string uri = list.last();
      string name = list.size() == 2 ? list.first() : list.last().split("//", 1L).last();
      list = side(1).split("; ");
      boolean link, title, caption;
      string width, height;
      for(auto p : list) {
        if(p == "link") { link = true; continue; }
        if(p == "title") { title = true; continue; }
        if(p == "caption") { caption = true; continue; }
        if(p.beginsWith("width:")) { p.trimLeft("width:", 1L); width = p.strip(); continue; }
        if(p.beginsWith("height:")) { p.trimLeft("height:", 1L); height = p.strip(); continue; }
      }

      if(caption) {
        t.append("<figure class='image'>\n");
        if(link) t.append("<a href=\"", escape(uri), "\">");
        t.append("<img loading=\"lazy\" src=\"", escape(uri), "\" alt=\"", escape(name ? name : uri.hash()), "\"");
        if(title) t.append(" title=\"", escape(name), "\"");
        if(width) t.append(" width=\"", escape(width), "\"");
        if(height) t.append(" height=\"", escape(height), "\"");
        t.append(">\n");
        if(link) t.append("</a>\n");
        t.append("<figcaption>", escape(name), "</figcaption>\n");
        t.append("</figure>");
      } else {
        if(link) t.append("<a href=\"", escape(uri), "\">");
        t.append("<img loading=\"lazy\" src=\"", escape(uri), "\" alt=\"", escape(name ? name : uri.hash()), "\"");
        if(title) t.append(" title=\"", escape(name), "\"");
        if(width && height) t.append(" style=\"width: ", escape(width), "px; max-height: ", escape(height), "px;\"");
        if(width) t.append(" width=\"", escape(width), "\"");
        if(height) t.append(" height=\"", escape(height), "\"");
        t.append(">");
        if(link) t.append("</a>");
      }

      n += 2;
      image = nothing;
      continue;
    }

    if(link || image) { n++; continue; }
    if(a =='\\') { t.append(b); n += 2; continue; }
    if(a == '&') { t.append("&amp;"); n++; continue; }
    if(a == '<') { t.append("&lt;"); n++; continue; }
    if(a == '>') { t.append("&gt;"); n++; continue; }
    if(a == '"') { t.append("&quot;"); n++; continue; }
    t.append(a); n++; continue;
  }

  if(strong) t.append("</strong>");
  if(emphasis) t.append("</em>");
  if(insertion) t.append("</ins>");
  if(deletion) t.append("</del>");
  if(code) t.append("</code>");

  return t;
}

}
