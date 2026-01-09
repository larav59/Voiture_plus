create table if not exists supervision.ALARMS_TYPES
(
    id        int auto_increment
        primary key,
    label     varchar(255) null,
    criticity varchar(255) null
)
    auto_increment = 5;

create table if not exists supervision.NODES_TYPES
(
    id    int auto_increment
        primary key,
    label varchar(255) null
)
    auto_increment = 4;

create table if not exists supervision.NODES
(
    id                int auto_increment
        primary key,
    name              varchar(255) null,
    position_x        float        null,
    position_y        float        null,
    point_of_interest tinyint(1)   null,
    offset_x          float        null,
    offset_y          float        null,
    node_type_id      int          null,
    constraint NODES_ibfk_1
        foreign key (node_type_id) references supervision.NODES_TYPES (id)
)
    auto_increment = 40;

create table if not exists supervision.ARCS
(
    id                  int auto_increment
        primary key,
    weight              int null,
    origin_node_id      int null,
    destination_node_id int null,
    type                int null,
    constraint ARCS_ibfk_1
        foreign key (origin_node_id) references supervision.NODES (id),
    constraint ARCS_ibfk_2
        foreign key (destination_node_id) references supervision.NODES (id)
)
    auto_increment = 25;

create index destination_node_id
    on supervision.ARCS (destination_node_id);

create index origin_node_id
    on supervision.ARCS (origin_node_id);

create index node_type_id
    on supervision.NODES (node_type_id);

create table if not exists supervision.ORIGINS
(
    id    int auto_increment
        primary key,
    label varchar(255) null
)
    auto_increment = 5;

create table if not exists supervision.ALARMS
(
    id            int auto_increment
        primary key,
    created_at    timestamp    null,
    description   varchar(255) null,
    alarm_type_id int          null,
    origin_id     int          null,
    constraint ALARMS_ibfk_1
        foreign key (alarm_type_id) references supervision.ALARMS_TYPES (id),
    constraint ALARMS_ibfk_2
        foreign key (origin_id) references supervision.ORIGINS (id)
)
    auto_increment = 45526;

create index alarm_type_id
    on supervision.ALARMS (alarm_type_id);

create index origin_id
    on supervision.ALARMS (origin_id);

create table if not exists supervision.USERS
(
    id       int auto_increment
        primary key,
    username varchar(255) null,
    password varchar(255) null
)
    auto_increment = 3;

create table if not exists supervision.VEHICLES
(
    id   int auto_increment
        primary key,
    name varchar(255) null
)
    auto_increment = 6;

create table if not exists supervision.STATES
(
    id         int auto_increment
        primary key,
    speed      float     null,
    angle      float     null,
    position_x float     null,
    position_y float     null,
    occured_at timestamp null,
    vehicle_id int       null,
    constraint STATES_ibfk_1
        foreign key (vehicle_id) references supervision.VEHICLES (id)
)
    auto_increment = 19249;

create index vehicle_id
    on supervision.STATES (vehicle_id);

create table if not exists supervision.TRAVELS
(
    id                 int auto_increment
        primary key,
    created_at         timestamp    null,
    status             varchar(255) null,
    estimated_duration float        null,
    vehicle_id         int          null,
    ordered_by         varchar(255) null,
    constraint TRAVELS_ibfk_1
        foreign key (vehicle_id) references supervision.VEHICLES (id)
)
    auto_increment = 53;

create index vehicle_id
    on supervision.TRAVELS (vehicle_id);

create table if not exists supervision.TRAVELS_NODES
(
    id         int auto_increment
        primary key,
    travel_id  int       null,
    node_id    int       null,
    `order`    int       null,
    arrived_at timestamp null,
    constraint TRAVELS_NODES_ibfk_1
        foreign key (travel_id) references supervision.TRAVELS (id),
    constraint TRAVELS_NODES_ibfk_2
        foreign key (node_id) references supervision.NODES (id)
)
    auto_increment = 81;

create index node_id
    on supervision.TRAVELS_NODES (node_id);

create index travel_id
    on supervision.TRAVELS_NODES (travel_id);


