@echo off
SetLocal EnableDelayedExpansion
(set PATH=F:\Qt\6.12.0\llvm-mingw_64\bin;!PATH!)
if defined QT_PLUGIN_PATH (
    set QT_PLUGIN_PATH=F:\Qt\6.12.0\llvm-mingw_64\plugins;!QT_PLUGIN_PATH!
) else (
    set QT_PLUGIN_PATH=F:\Qt\6.12.0\llvm-mingw_64\plugins
)
%*
EndLocal
