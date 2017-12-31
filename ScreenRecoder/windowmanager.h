#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

class RecorderWindow;
class EditorWindow;
class OutputWindow;

class WindowManager
{
private:
  RecorderWindow* recorderWindow_;
  EditorWindow* editorWindow_;
  OutputWindow* outputWindow_;
  WindowManager();
  static WindowManager _instance;
public:
  static WindowManager& instance();
  static RecorderWindow& getRecorderWindow();
  static EditorWindow& getEditorWindow();
  static OutputWindow& getOuputWindow();
  ~WindowManager();
};

#endif // WINDOWMANAGER_H
