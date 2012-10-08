create table 
if not exists
version
(
id INTEGER PRIMARY KEY,
name VARCHAR(20),
vers INTEGER
);

create table 
if not exists
users
(
id INTEGER PRIMARY KEY,
active INTEGER,
name VARCHAR(48),
pswd VARCHAR(64),
used INTEGER
);

create table
if not exists
devices
(
id INTEGER PRIMARY KEY,
serial VARCHAR(24)
);

create table
if not exists
standards
(
id INTEGER PRIMARY KEY,
txt VARCHAR(24)
);

create table
if not exists
gritSizes
(
id INTEGER PRIMARY KEY,
standard INTEGER,
gritIndex INTEGER,
txt VARCHAR(24)
);

create table
if not exists
sessions
(
id INTEGER PRIMARY KEY,
user INTEGER,
device INTEGER,
start DATETIME,
end DATETIME,
lot VARCHAR(24),
standard INTEGER,
grit INTEGER,
mark VARCHER(24)
);

create table 
if not exists
measures
(
id INTEGER PRIMARY KEY,
session INTEGER,
size REAL,
strenght REAL,
elapsed INTEGER,
ignored INTEGER
);
