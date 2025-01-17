# CSCN73060_25W_Sec3_Group2_WebProject
The repository for Group 2's Web Project for eficiency testing


DATABASE SETUP
...................................................................................................................

https://www.sqlite.org/download.html

download: 
Precompiled Binaries for Windows
sqlite-tools-win-x64-3480000.zip
(5.25 MiB)

extract where ever desired

next step:

Press Win + R, type sysdm.cpl, and press Enter.
In the System Properties window, go to the Advanced tab.
Click on Environment Variables.
Under System variables, find and select the Path variable, then click Edit....
Click New and enter the path to the SQLite folder (e.g., C:\sqlite).
Click OK to save and close all windows.

next step:

open command prompt

sqlite3

- should give u version n all (it means its installed properly)




PROJECT SETUP
....................................................................................................................

docker run --rm -ti -p 23500:23500 -v "C:\Users\mujta\OneDrive\Desktop\Semester 6\Project 6\CSCN73060_25W_Sec3_Group2_WebProject-main\Group2WebProject:/Shared" webserver_dev bash

cd /Shared

mkdir build

cd build

apt update

apt-get install libasio-dev

apt-get install -y nlohmann-json3-dev

apt-get install -y sqlite3 libsqlite3-dev

cmake ..

make

./Group2WebProject

http://localhost:23500/


