@echo off
SET logFolder=%1
IF NOT EXIST %logFolder% (mkdir "%logFolder%")
