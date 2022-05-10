#pragma once

#define INFO_COUNT 15

void noprint(u8 r);

void InfoPrintSeparatorLine();

void InfoPrintFileNameLabel();

void InfoPrintFileNameValue();

void InfoPrintFramesCount();

void InfoPrintLockState();

void InfoPrintFramePlaybackSpeed();

void InfoPrintRecordingFramePlaybackSpeed();

void infoPrintSoundFrequency();

void infoPrintSoundSize();

extern void (*InfoLine[INFO_COUNT])();

extern u8 InfoScrollPos;

void InfoDisplay();
