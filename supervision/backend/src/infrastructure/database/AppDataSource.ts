import { DataSource } from "typeorm";
import { mysqlConfig } from "../../config";
import { Alarms } from "../../domain/entities/Alarms";
import { AlarmsTypes } from "../../domain/entities/AlarmsTypes";
import { Arcs } from "../../domain/entities/Arcs";
import { Nodes } from "../../domain/entities/Nodes";
import { NodesTypes } from "../../domain/entities/NodesTypes";
import { Origins } from "../../domain/entities/Origins";
import { States } from "../../domain/entities/States";
import { Travels } from "../../domain/entities/Travels";
import { TravelsNodes } from "../../domain/entities/TravelsNodes";
import { Users } from "../../domain/entities/Users";
import { Vehicles } from "../../domain/entities/Vehicles";


const AppDataSource = new DataSource({
	type: "mysql",
	host: mysqlConfig.host,
	port: mysqlConfig.port, 
	username: mysqlConfig.user,
	password: mysqlConfig.password,
	database: mysqlConfig.database,
	charset: mysqlConfig.charset,
	synchronize: false,
	entitySkipConstructor: true,
	logging: false , //mysqlConfig.logging,
	entities: [
		Alarms,
		AlarmsTypes,
		Arcs,
		Nodes,
		NodesTypes,
		Origins,
		States,
		Travels,
		TravelsNodes,
		Users,
		Vehicles
	],
	subscribers: [],
});

export { AppDataSource };