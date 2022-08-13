#include "test_eventflag.hpp"

int test_eventflag(int argc, char *argv[])
{
    TEST_OTP::EventFlag testObject;
    return QTest::qExec(&testObject, argc, argv);
}

void TEST_OTP::EventFlag::initTestCase()
{}

void TEST_OTP::EventFlag::constuctor()
{
    {
        ::EventFlag ev;
        QCOMPARE(ev, false);
    }

    {
        ::EventFlag ev(false);
        QCOMPARE(ev, false);
    }

    {
        ::EventFlag ev(true);
        QCOMPARE(ev, true);
    }
}

void TEST_OTP::EventFlag::assignmentOperator()
{
    ::EventFlag ev;

    ev = true;
    QCOMPARE(ev, true);

    ev = false;
    QCOMPARE(ev, false);
}

TEST_OTP::EventFlag::WaitHelper::WaitHelper(::EventFlag &ev, bool target, std::chrono::milliseconds waitTime)
{
    const auto preTimeout = waitTime - ((waitTime/100) * 10); // waitTime - 10%
    const auto timeout = waitTime + ((waitTime/100) * 10); // waitTime + 10%

    // Set target after timeout
    auto aSyncSetTarget = QtConcurrent::run([this, &ev, target, waitTime]{
        this->semaphore.acquire();
        QThread::msleep(std::chrono::duration_cast<std::chrono::milliseconds>(waitTime).count());
        ev = target;
        this->semaphore.release();
    });

    // Check value is unmodified before timeout
    auto aSyncPreTimeout = QtConcurrent::run([this, &ev, target, preTimeout]{
        this->semaphore.acquire();
        QThread::msleep(std::chrono::duration_cast<std::chrono::milliseconds>(preTimeout).count());
        QCOMPARE(ev, !target);
        this->semaphore.release();
    });

    // Check value is modified after timeout
    auto aSyncTimeout = QtConcurrent::run([this, &ev, target, timeout]{
        this->semaphore.acquire();
        QThread::msleep(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
        QCOMPARE(ev, target);
        this->semaphore.release();
    });
}

void TEST_OTP::EventFlag::WaitHelper::waitComplete()
{
    while (semaphore.available() < semaphoreCount)
        QCoreApplication::processEvents();
}

void TEST_OTP::EventFlag::wait()
{
    using namespace std::literals::chrono_literals;
    const auto waitTime = 1000ms;
    bool target = true;
    ::EventFlag ev(!target);

    // Test wait true
    {
        auto wh = WaitHelper(ev, target, waitTime);
        QCOMPARE(ev, !target);
        ev.wait(target);
        QCOMPARE(ev, target);
        wh.waitComplete();
    }

    // Test wait false
    {
        target = !target;
        auto wh = WaitHelper(ev, target, waitTime);
        QCOMPARE(ev, !target);
        ev.wait(target);
        QCOMPARE(ev, target);
        wh.waitComplete();
    }

    // Test already is true
    {
        target = !target;
        ev = target;
        QCOMPARE(ev, target);
        ev.wait(target);
        QCOMPARE(ev, target);
    }

    // Test already is false
    {
        target = !target;
        ev = target;
        QCOMPARE(ev, target);
        ev.wait(target);
        QCOMPARE(ev, target);
    }
}

void TEST_OTP::EventFlag::waitfor()
{
    using namespace std::literals::chrono_literals;
    const auto waitTime = 1000ms;
    bool target = true;
    bool result = false;
    ::EventFlag ev(!target);

    // Test waitFor true
    {
        auto wh = WaitHelper(ev, target, waitTime);
        QCOMPARE(ev, !target);
        result = ev.waitFor(target, waitTime * 2);
        QVERIFY(result);
        QCOMPARE(ev, target);
        wh.waitComplete();
    }

    // Test waitFor false
    {
        target = !target;
        auto wh = WaitHelper(ev, target, waitTime);
        QCOMPARE(ev, !target);
        result = ev.waitFor(target, waitTime * 2);
        QVERIFY(result);
        QCOMPARE(ev, target);
        wh.waitComplete();
    }

    // Test waitFor true timeout
    {
        target = !target;
        ev = !target;
        result = ev.waitFor(target, waitTime);
        QCOMPARE(result, false);
    }

    // Test waitFor false timeout
    {
        target = !target;
        ev = !target;
        result = ev.waitFor(target, waitTime);
        QCOMPARE(result, false);
    }

    // Test already is true
    {
        target = !target;
        ev = target;
        QCOMPARE(ev, target);
        result = ev.waitFor(target, waitTime);
        QVERIFY(result);
        QCOMPARE(ev, target);
    }

    // Test already is false
    {
        target = !target;
        ev = target;
        QCOMPARE(ev, target);
        result = ev.waitFor(target, waitTime);
        QVERIFY(result);
        QCOMPARE(ev, target);
    }
}
