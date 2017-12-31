#include "windowmanager.h"
#include "editorwindow.h"
#include "outputwindow.h"
#include "recorderwindow.h"
WindowManager WindowManager::_instance;

WindowManager::WindowManager()
    : recorderWindow_(nullptr),
      editorWindow_(nullptr),
      outputWindow_(nullptr) {}

WindowManager &WindowManager::instance() { return _instance; }

RecorderWindow &WindowManager::getRecorderWindow() {
  if (_instance.recorderWindow_ == nullptr) {
    _instance.recorderWindow_ = new RecorderWindow();
  }
  return *_instance.recorderWindow_;
}

EditorWindow &WindowManager::getEditorWindow() {
  if (_instance.editorWindow_ == nullptr) {
    _instance.editorWindow_ = new EditorWindow();
  }
  return *_instance.editorWindow_;
}

OutputWindow &WindowManager::getOuputWindow() {
  if (_instance.outputWindow_ == nullptr) {
    _instance.outputWindow_ = new OutputWindow();
  }
  return *_instance.outputWindow_;
}

WindowManager::~WindowManager() {
  if (recorderWindow_ == nullptr) delete recorderWindow_;
  if (editorWindow_ == nullptr) delete editorWindow_;
  if (outputWindow_ == nullptr) delete outputWindow_;
}
