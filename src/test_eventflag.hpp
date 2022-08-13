#ifndef TEST_EVENTFLAG_HPP
#define TEST_EVENTFLAG_HPP

#include <QtTest/QTest>

#include "eventflag.hpp"
#include <QtConcurrent>

namespace TEST_OTP
{
    class EventFlag : public QObject
    {
        Q_OBJECT

    public:
        EventFlag() = default;
        ~EventFlag() = default;

    private slots:
        void initTestCase();

        void constuctor();
        void assignmentOperator();
        void wait();
        void waitfor();

    private:
        /**
         * @brief Wait() and WaitFor() helper class
         */
        class WaitHelper
        {
        public:
            /**
             * @brief Three helper actions
             *        A) After waitTimer, set EventFlag to the target value
             *        B) Check at waitTime-10%, that the target has not already been set
             *        C) Check at waitTime+10%, that the target is still set
             *
             * @param ev EventFlag under test
             * @param target Target value for EventFlag
             * @param waitTime Wait time until setting target
             */
            WaitHelper(::EventFlag &ev, bool target, std::chrono::milliseconds waitTime);

            /**
             * @brief Wait until all the helper actions are complete
             */
            void waitComplete();

        private:
            /**
             * @brief Number of actions to track with semaphore
             */
            const int semaphoreCount = 3;

            /**
             * @brief Active actions counter
             */
            QSemaphore semaphore = QSemaphore(semaphoreCount);
        };
    };
}

#endif // TEST_EVENTFLAG_HPP
