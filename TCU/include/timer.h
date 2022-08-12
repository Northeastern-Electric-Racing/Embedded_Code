/**
 * @file timer.h
 * @author Nick DePatie
 * @brief Standardizes the Use of Timers using NERduino
 * @date 2022-03-25
 */

#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

class Timer
{
    private:
        bool running = 0;
        uint32_t expireTime;

    public:
        Timer();
        
        ~Timer();

        /**
         * @brief Starts a timer for the inputted timeout value in milliseconds
         * 
         * @param timeout (milliseconds)
         */
        void startTimer(uint32_t timeout);

        /**
         * @brief Sets the time for the timer to expire at the current time, effectively canceling the timer
         * 
         */
        void cancelTimer();

        /**
         * @brief Checks to see if the timer is still running
         * 
         * @return true 
         * @return false 
         */
        bool isTimerExpired();
};

#endif