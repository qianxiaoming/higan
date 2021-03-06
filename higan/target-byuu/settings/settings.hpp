struct Settings : Markup::Node {
  using string = nall::string;

  auto load() -> void;
  auto save() -> void;
  auto process(bool load) -> void;

  struct Video {
    string driver;
    string monitor;
    string format;
    bool exclusive = false;
    bool blocking = false;
    bool flush = false;
    string shader = "Blur";
    uint multiplier = 2;
    string output = "Scale";
    bool aspectCorrection = true;
    bool adaptiveSizing = true;
    bool autoCentering = false;

    double luminance = 1.0;
    double saturation = 1.0;
    double gamma = 1.0;
    bool colorBleed = true;
    bool colorEmulation = true;
    bool interframeBlending = true;
    bool overscan = false;
  } video;

  struct Audio {
    string driver;
    string device;
    uint frequency = 0;
    uint latency = 0;
    bool exclusive = false;
    bool blocking = true;
    bool dynamic = false;
    bool mute = false;

    double volume = 1.0;
    double balance = 0.0;
  } audio;

  struct Input {
    string driver;
    string defocus = "Pause";
  } input;

  struct General {
    bool showStatusBar = true;
  } general;

  struct Recent {
    string game[9];
  } recent;
};

struct VideoSettings : VerticalLayout {
  auto construct() -> void;

  Label colorAdjustmentLabel{this, Size{~0, 0}, 2};
  TableLayout colorAdjustmentLayout{this, Size{~0, 0}};
    Label luminanceLabel{&colorAdjustmentLayout, Size{0, 0}};
    Label luminanceValue{&colorAdjustmentLayout, Size{50_sx, 0}};
    HorizontalSlider luminanceSlider{&colorAdjustmentLayout, Size{~0, 0}};
  //
    Label saturationLabel{&colorAdjustmentLayout, Size{0, 0}};
    Label saturationValue{&colorAdjustmentLayout, Size{50_sx, 0}};
    HorizontalSlider saturationSlider{&colorAdjustmentLayout, Size{~0, 0}};
  //
    Label gammaLabel{&colorAdjustmentLayout, Size{0, 0}};
    Label gammaValue{&colorAdjustmentLayout, Size{50_sx, 0}};
    HorizontalSlider gammaSlider{&colorAdjustmentLayout, Size{~0, 0}};
  Label emulatorOptionsLabel{this, Size{~0, 0}, 2};
    CheckLabel colorBleedOption{this, Size{0, 0}, 2};
    CheckLabel colorEmulationOption{this, Size{0, 0}, 2};
    CheckLabel interframeBlendingOption{this, Size{0, 0}, 2};
    CheckLabel overscanOption{this, Size{0, 0}};
};

struct AudioSettings : VerticalLayout {
  auto construct() -> void;

  Label effectsLabel{this, Size{~0, 0}, 2};
  TableLayout effectsLayout{this, Size{~0, 0}};
    Label volumeLabel{&effectsLayout, Size{0, 0}};
    Label volumeValue{&effectsLayout, Size{50_sx, 0}};
    HorizontalSlider volumeSlider{&effectsLayout, Size{~0, 0}};
  //
    Label balanceLabel{&effectsLayout, Size{0, 0}};
    Label balanceValue{&effectsLayout, Size{50_sx, 0}};
    HorizontalSlider balanceSlider{&effectsLayout, Size{~0, 0}};
};

struct InputSettings : VerticalLayout {
  auto construct() -> void;
  auto reload() -> void;
  auto refresh() -> void;
  auto eventChange() -> void;
  auto eventClear() -> void;
  auto eventAssign() -> void;
  auto eventInput(shared_pointer<HID::Device>, uint groupID, uint inputID, int16_t oldValue, int16_t newValue) -> void;

  TableView inputList{this, Size{~0, ~0}};
  HorizontalLayout controlLayout{this, Size{~0, 0}};
    Widget spacer{&controlLayout, Size{~0, 0}};
    Button assignButton{&controlLayout, Size{80, 0}};
    Button clearButton{&controlLayout, Size{80, 0}};

  maybe<InputMapping&> activeMapping;
};

struct HotkeySettings : VerticalLayout {
  auto construct() -> void;
  auto reload() -> void;
  auto refresh() -> void;
  auto eventChange() -> void;
  auto eventClear() -> void;
  auto eventAssign() -> void;
  auto eventInput(shared_pointer<HID::Device>, uint groupID, uint inputID, int16_t oldValue, int16_t newValue) -> void;

  TableView inputList{this, Size{~0, ~0}};
  HorizontalLayout controlLayout{this, Size{~0, 0}};
    Widget spacer{&controlLayout, Size{~0, 0}};
    Button assignButton{&controlLayout, Size{80, 0}};
    Button clearButton{&controlLayout, Size{80, 0}};

  maybe<InputMapping&> activeMapping;
};

struct EmulatorSettings : VerticalLayout {
  auto construct() -> void;
  auto eventToggle(TableViewCell cell) -> void;

  Label emulatorLabel{this, Size{~0, 0}, 2};
  TableView emulatorList{this, Size{~0, ~0}};
};

struct DriverSettings : VerticalLayout {
  auto construct() -> void;
  auto videoRefresh() -> void;
  auto videoDriverUpdate() -> void;
  auto audioRefresh() -> void;
  auto audioDriverUpdate() -> void;
  auto inputRefresh() -> void;
  auto inputDriverUpdate() -> void;

  Label videoLabel{this, Size{~0, 0}, 2};
  HorizontalLayout videoDriverLayout{this, Size{~0, 0}};
    Label videoDriverLabel{&videoDriverLayout, Size{0, 0}};
    ComboButton videoDriverList{&videoDriverLayout, Size{0, 0}};
    Button videoDriverAssign{&videoDriverLayout, Size{0, 0}};
    Label videoDriverActive{&videoDriverLayout, Size{0, 0}};
  HorizontalLayout videoPropertyLayout{this, Size{~0, 0}};
    Label videoMonitorLabel{&videoPropertyLayout, Size{0, 0}};
    ComboButton videoMonitorList{&videoPropertyLayout, Size{0, 0}};
    Label videoFormatLabel{&videoPropertyLayout, Size{0, 0}};
    ComboButton videoFormatList{&videoPropertyLayout, Size{0, 0}};
  HorizontalLayout videoToggleLayout{this, Size{~0, 0}};
    CheckLabel videoExclusiveToggle{&videoToggleLayout, Size{0, 0}};
    CheckLabel videoBlockingToggle{&videoToggleLayout, Size{0, 0}};
    CheckLabel videoFlushToggle{&videoToggleLayout, Size{0, 0}};
  //
  Label audioLabel{this, Size{~0, 0}, 2};
  HorizontalLayout audioDriverLayout{this, Size{~0, 0}};
    Label audioDriverLabel{&audioDriverLayout, Size{0, 0}};
    ComboButton audioDriverList{&audioDriverLayout, Size{0, 0}};
    Button audioDriverAssign{&audioDriverLayout, Size{0, 0}};
    Label audioDriverActive{&audioDriverLayout, Size{0, 0}};
  HorizontalLayout audioPropertyLayout{this, Size{~0, 0}};
    Label audioDeviceLabel{&audioPropertyLayout, Size{0, 0}};
    ComboButton audioDeviceList{&audioPropertyLayout, Size{0, 0}};
    Label audioFrequencyLabel{&audioPropertyLayout, Size{0, 0}};
    ComboButton audioFrequencyList{&audioPropertyLayout, Size{0, 0}};
    Label audioLatencyLabel{&audioPropertyLayout, Size{0, 0}};
    ComboButton audioLatencyList{&audioPropertyLayout, Size{0, 0}};
  HorizontalLayout audioToggleLayout{this, Size{~0, 0}};
    CheckLabel audioExclusiveToggle{&audioToggleLayout, Size{0, 0}};
    CheckLabel audioBlockingToggle{&audioToggleLayout, Size{0, 0}};
    CheckLabel audioDynamicToggle{&audioToggleLayout, Size{0, 0}};
  //
  Label inputLabel{this, Size{~0, 0}, 2};
  HorizontalLayout inputDriverLayout{this, Size{~0, 0}};
    Label inputDriverLabel{&inputDriverLayout, Size{0, 0}};
    ComboButton inputDriverList{&inputDriverLayout, Size{0, 0}};
    Button inputDriverAssign{&inputDriverLayout, Size{0, 0}};
    Label inputDriverActive{&inputDriverLayout, Size{0, 0}};
  HorizontalLayout inputDefocusLayout{this, Size{~0, 0}};
    Label inputDefocusLabel{&inputDefocusLayout, Size{0, 0}};
    RadioLabel inputDefocusPause{&inputDefocusLayout, Size{0, 0}};
    RadioLabel inputDefocusBlock{&inputDefocusLayout, Size{0, 0}};
    RadioLabel inputDefocusAllow{&inputDefocusLayout, Size{0, 0}};
    Group inputDefocusGroup{&inputDefocusPause, &inputDefocusBlock, &inputDefocusAllow};
};

struct SettingsWindow : Window {
  SettingsWindow();
  auto show(const string& panel) -> void;
  auto eventChange() -> void;

  HorizontalLayout layout{this};
    ListView panelList{&layout, Size{125_sx, ~0}};
    VerticalLayout panelContainer{&layout, Size{~0, ~0}};
      VideoSettings videoSettings;
      AudioSettings audioSettings;
      InputSettings inputSettings;
      HotkeySettings hotkeySettings;
      EmulatorSettings emulatorSettings;
      DriverSettings driverSettings;

  StatusBar statusBar{this};
};

extern Settings settings;
namespace Instances { extern Instance<SettingsWindow> settingsWindow; }
extern SettingsWindow& settingsWindow;
extern VideoSettings& videoSettings;
extern AudioSettings& audioSettings;
extern InputSettings& inputSettings;
extern HotkeySettings& hotkeySettings;
extern EmulatorSettings& emulatorSettings;
extern DriverSettings& driverSettings;
