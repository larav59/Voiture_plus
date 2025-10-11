import { DataSource } from "typeorm";
import { mysqlConfig } from "../../config";


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
	logging: mysqlConfig.logging,
	entities: [
		// Ajouter ici les entités
	],
	subscribers: [],
});

export { AppDataSource };