do $$
begin
	WHILE( EXISTS(SELECT 1 FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS WHERE CONSTRAINT_TYPE = 'FOREIGN KEY') ) LOOP

	PREPARE sql_alterTable_1 AS
	SELECT ('ALTER TABLE ' || table_schema || '.[' || TABLE_NAME || '] DROP CONSTRAINT [' || constraint_name || ']')
	FROM information_schema.table_constraints
	WHERE CONSTRAINT_TYPE = 'FOREIGN KEY'
	LIMIT 1;
	execute sql_alterTable_1;

	end loop;
end$$;


DROP TABLE IF EXISTS log;
CREATE TABLE log (
	id bigint NOT NULL,
	name varchar(250) NULL,
	CONSTRAINT log_PK PRIMARY KEY (id)
);


DROP TABLE IF EXISTS classifier;
CREATE TABLE classifier (
	id bigint NOT NULL,
	name varchar(50) NOT NULL,
	keys varchar(250) NOT NULL,
	log_id bigint NOT NULL,
	CONSTRAINT classifier_PK PRIMARY KEY (id),
	CONSTRAINT classifier_FK_log_id FOREIGN KEY (log_id) REFERENCES log(id)
);

DROP TABLE IF EXISTS extension;
CREATE TABLE extension (
	id bigint NOT NULL,
	name varchar(50) NOT NULL,
	prefix varchar(50) NOT NULL,
	uri varchar(250) NOT NULL,
	CONSTRAINT extension_PK PRIMARY KEY (id)
);

DROP TABLE IF EXISTS attribute;
CREATE TABLE attribute (
	id bigint NOT NULL,
	type varchar(50) NOT NULL,
	key varchar(50) NOT NULL,
	ext_id bigint NULL,
	parent_id bigint NULL,
	CONSTRAINT attribute_PK PRIMARY KEY (id),
	CONSTRAINT attribute_FK_ext_id FOREIGN KEY (ext_id) REFERENCES extension(id),
	CONSTRAINT attribute_FK_parent_id FOREIGN KEY (parent_id) REFERENCES attribute(id)
);

DROP TABLE IF EXISTS log_has_attribute;
CREATE TABLE log_has_attribute (
	log_id bigint NOT NULL,
	trace_global bit NOT NULL,
	event_global bit NOT NULL,
	attr_id bigint NOT NULL,
	value varchar(250) NOT NULL,
	CONSTRAINT log_has_attribute_PK PRIMARY KEY (log_id,trace_global,event_global,attr_id),
	CONSTRAINT log_has_attribute_FK_attr_id FOREIGN KEY (attr_id) REFERENCES attribute(id),
	CONSTRAINT log_has_attribute_FK_log_id FOREIGN KEY (log_id) REFERENCES log(id)
);

DROP TABLE IF EXISTS trace;
CREATE TABLE trace (
	id bigint NOT NULL,
	name varchar(250) NULL,
	log_id bigint NOT NULL,
	CONSTRAINT trace_PK PRIMARY KEY (id),
	CONSTRAINT trace_FK_log_id FOREIGN KEY (log_id) REFERENCES log(id)
);



DROP TABLE IF EXISTS trace_has_attribute;
CREATE TABLE trace_has_attribute (
	trace_id bigint NOT NULL,
	attr_id bigint NOT NULL,
	value varchar(250) NOT NULL,
	CONSTRAINT trace_has_attribute_PK PRIMARY KEY (trace_id,attr_id),
	CONSTRAINT trace_has_attribute_FK_attr_id FOREIGN KEY (attr_id) REFERENCES attribute(id),
	CONSTRAINT trace_has_attribute_FK_trace_id FOREIGN KEY (trace_id) REFERENCES trace(id)
);

DROP TABLE IF EXISTS event_collection;
CREATE TABLE event_collection (
	id bigint NOT NULL,
	name varchar(50) NULL,
	CONSTRAINT event_collection_PK PRIMARY KEY (id)
);


DROP TABLE IF EXISTS event;
CREATE TABLE event (
	id bigint NOT NULL,
	trace_id bigint NOT NULL,
	name varchar(250) NOT NULL,
	transition varchar(50) NOT NULL,
	event_coll_id bigint NULL,
	time timestamp(3) NOT NULL,
	CONSTRAINT event_PK PRIMARY KEY (id),
	CONSTRAINT event_FK_event_coll_id FOREIGN KEY (event_coll_id) REFERENCES event_collection(id),
	CONSTRAINT event_FK_trace_id FOREIGN KEY (trace_id) REFERENCES trace(id)
);

DROP TABLE IF EXISTS event_has_attribute;
CREATE TABLE event_has_attribute (
	event_id bigint NOT NULL,
	attr_id bigint NOT NULL,
	value varchar(250) NOT NULL,
	CONSTRAINT event_has_attribute_PK PRIMARY KEY (event_id,attr_id),
	CONSTRAINT event_has_attribute_FK_attr_id FOREIGN KEY (attr_id) REFERENCES attribute(id),
	CONSTRAINT event_has_attribute_FK_event_id FOREIGN KEY (event_id) REFERENCES event(id)
);
