create table if not exists marks (id INTEGER PRIMARY KEY, txt VARCHAR(24));

create table if not exists products (id INTEGER PRIMARY KEY, txt VARCHAR(24));

insert into products(id, txt) values(0, '');

insert into marks(txt) select distinct mark from sessions;

create temp table tmp(id INTEGER PRIMARY KEY,user INTEGER,device INTEGER,start DATETIME,end DATETIME,
lot VARCHAR(24),standard INTEGER,grit INTEGER,mark INTEGER,product INTEGER);

insert into tmp(id, user, device, start, end, lot, standard, grit, mark, product)
select s.id, s.user, s.device, s.start, s.end, s.lot, s.standard, s.grit, m.id, 0
from sessions s, marks m where s.mark = m.txt;

drop table sessions;

create table sessions(id INTEGER PRIMARY KEY,user INTEGER,device INTEGER,start DATETIME,end DATETIME,
lot VARCHAR(24),standard INTEGER,grit INTEGER,mark INTEGER,product INTEGER);

insert into sessions select * from tmp;

drop table tmp;
