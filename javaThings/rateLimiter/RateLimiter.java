package com.letmesee.www.core;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class RateLimiter {

    public static void main(String[] args) throws InterruptedException {
        RateLimiter test = new RateLimiter(5, DEFAULT_CAPACITY);
        Thread.sleep(1000);
        for (;;) {
            Thread.sleep(1000);
            boolean res = test.tryAcquire(10);
            System.out.println("本次获取结果: " + res);
        }
    }

    public static final int DEFAULT_CAPACITY = 1000;

    public static final double DEFAULT_LIMIT_SIZE = 10.0;

    private static final double SEC_TO_NS = 1e9;

    // 桶的总大小
    private int burstCapacity;

    // 桶中当前存量
    private double butstCurrent;

    // 每秒产生多少令牌
    private double limitSize = -1;

    // 最后一次获取令牌的时间戳 ns
    private long lastUpdateAt;

    // 下一次有令牌的时间戳 ns
    private long expectAt;

    private ReentrantLock lock = new ReentrantLock(false);

    private Condition cond = lock.newCondition();

    public RateLimiter() {
        this.limitSize = DEFAULT_LIMIT_SIZE;
        this.burstCapacity = DEFAULT_CAPACITY;
        this.lastUpdateAt = System.nanoTime();
        this.expectAt = System.nanoTime();
    }

    public RateLimiter(double limitSize, int burstCapacity) {
        this.limitSize = limitSize;
        this.burstCapacity = burstCapacity;
        this.lastUpdateAt = System.nanoTime();
        this.expectAt = System.nanoTime();
    }

    public int getBurstCapacity() {
        return burstCapacity;
    }

    public double getBurstCurrent() {
        return butstCurrent;
    }

    public double getLimitSize() {
        return limitSize;
    }

    public boolean acquire(int count, long maxWaitTimestamp) {
        return getTokens(System.nanoTime(), count, maxWaitTimestamp);
    }

    public boolean tryAcquire(int count) {
        return getTokens(System.nanoTime(), count, -1);
    }

    private boolean getTokens(long nowTimestamp, int count, long maxWaitTimestamp) {
        try {
            lock.lock();
            if (limitSize == -1) {
                // 如果没有设置限制，则直接返回
                return true;
            }

            while (nowTimestamp < expectAt) {
                // 如果前面预支过了并且没有到达下次期望时间
                long remain = expectAt - nowTimestamp;
                if (maxWaitTimestamp < remain) {
                    return false;
                }
                cond.awaitNanos(remain);
                nowTimestamp = System.nanoTime();
                maxWaitTimestamp -= remain;
            }

            // 更新一下当前令牌数量
            updateBurst(nowTimestamp);
            butstCurrent -= count;

            if (butstCurrent < 0) {
                // 说明当前令牌数量不够
                long needWaitTimestamp = RateLimiter.tokensToTime(-butstCurrent, limitSize);
                expectAt = nowTimestamp + needWaitTimestamp;
            } else {
                expectAt = nowTimestamp;
            }
            lastUpdateAt = nowTimestamp;
            return true;
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            lock.unlock();
        }
        return false;
    }

    /**
     * 更新桶令牌数量
     * 
     * @param nowTimestamp
     */
    private void updateBurst(long nowTimestamp) {
        lastUpdateAt = nowTimestamp < lastUpdateAt ? nowTimestamp : lastUpdateAt;
        long maxNeed = RateLimiter.tokensToTime(burstCapacity - butstCurrent, limitSize);
        long duration = Math.min(nowTimestamp - lastUpdateAt, maxNeed);
        butstCurrent += RateLimiter.timeToTokens(duration, limitSize);
        butstCurrent = Math.min((double) burstCapacity, butstCurrent);
    }

    /**
     * 通过token数量获取时间
     * 
     * @param tokens
     * @param limitSize
     * @return
     */
    private static long tokensToTime(double tokens, double limitSize) {
        double needSec = tokens / limitSize;
        return (long) (needSec * SEC_TO_NS);
    }

    /**
     * 通过时间获取token数量
     * 
     * @param ns
     * @return
     */
    private static double timeToTokens(long ns, double limitSize) {
        return ns / SEC_TO_NS * limitSize;
    }
}