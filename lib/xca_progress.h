#ifndef XCA_PROGRESS_H
#define XCA_PROGRESS_H

#include <QString>

class XcaProgress_i {
 public:
  XcaProgress_i() = default;
  virtual void start(const QString& what, int max) = 0;
  virtual void stop() = 0;
  virtual void increment() = 0;
  virtual ~XcaProgress_i() = default;
};

class XcaProgressCmd final : public XcaProgress_i {
 private:
  int i;

 public:
  void start(const QString& what, int max) final;
  void stop() final;
  void increment() final;
};

class XcaProgress {
 private:
  static XcaProgress_i* progress;

 public:
  XcaProgress(const QString& what = QString(), int max = 100);
  ~XcaProgress();
  static void increment();

  static void inc(int, int, void* p);
  static void setGui(XcaProgress_i* p);
};

#endif  // XCA_PROGRESS_H
