#ifndef GPIOWORKERTHREAD_H
#define GPIOWORKERTHREAD_H
#include <QThread>

class GPIOWorkerThread : public QThread
{
    Q_OBJECT
public:
    enum State{
        NoState,
        GateClosed,
        GateOpen,
        ClosedState
    };
    Q_ENUM(State)
    void setState(State state);
    void run() Q_DECL_OVERRIDE;
signals:
    void closingPin(int pin);
    void stateChanged(State state);

private:
    static void msleep(unsigned long msecs);
    State m_state=NoState;
    bool btn_is_press;
    bool onClosing(int pin);
};

#endif // GPIOWORKERTHREAD_H
