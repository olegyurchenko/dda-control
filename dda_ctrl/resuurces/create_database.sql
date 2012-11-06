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
grits
(
id INTEGER PRIMARY KEY,
standard INTEGER,
gritIndex INTEGER,
meshIndex INTEGER,
txt VARCHAR(24)
);

create table
if not exists
marks
(
id INTEGER PRIMARY KEY,
txt VARCHAR(24)
);

create table
if not exists
products
(
id INTEGER PRIMARY KEY,
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
mark INTEGER,
product INTEGER
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

insert or replace into version(id, name, vers) values(0, 'SESSIONS', 1);
insert or replace into version(id, name, vers) values(1, 'USERS', 1);
insert or replace into version(id, name, vers) values(2, 'DEVICES', 1);
insert or replace into version(id, name, vers) values(3, 'STANDARDS', 1);
insert or replace into version(id, name, vers) values(4, 'GRITS', 1);
insert or replace into version(id, name, vers) values(5, 'MARKS', 1);
insert or replace into version(id, name, vers) values(6, 'PRODUCTS', 1);
insert or replace into version(id, name, vers) values(7, 'MEASURES', 1);
