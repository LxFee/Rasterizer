@echo off
SET Folder=%1
IF NOT EXIST %Folder% (mkdir "%Folder%" 2>nul)
