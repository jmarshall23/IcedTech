for /R %cd% %%G IN (*.ogg) do (
	..\..\tools\freac-1.0.32-bin\freaccmd.exe -e WAVE %%G -o %%~pG%%~nG.wav
	del %%G )

pause