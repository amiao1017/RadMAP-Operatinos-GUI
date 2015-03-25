SetBatchLines, -1  ; Make the operation run at maximum speed.
SetTitleMatchMode 2

ifWinExist, startArduino

Folder = %clipboard%
Loop, %Folder%\*.*, , 1
	FolderSize += %A_LoopFileSize%
clipboard = %FolderSize%
	
;MsgBox Size of %arduinoFolder% is %arduinoFolderSize% bytes.
