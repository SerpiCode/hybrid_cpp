@echo off
REM target-runner.bat <instance_id> <config_id> <seed> <instance_path> <rest>
REM Passa seed e instance_file nas flags corretas
set SEED=%3
set INSTANCE=%4

REM Chama o executável repassando o resto dos argumentos (%*) que inclui --popsize, --elitefrac, --mutantfrac
main-irace.exe --instance "%INSTANCE%" --seed %SEED% %*
exit /b %ERRORLEVEL%
