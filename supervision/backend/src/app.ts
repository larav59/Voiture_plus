import dotenv from "dotenv";
dotenv.config({ quiet: true });

import express, { Router } from "express";
import { WebApiServer } from "./infrastructure/webserver/WebApiServer";
import { AppDataSource } from "./infrastructure/database/AppDataSource";
import { logger } from "./infrastructure/loggers";
import { expressConfig } from "./config";


import apiRoutes from "./routes/index";
import { errorHandlerMiddleware } from "./middlewares/errorHandlerMiddleware";


const app = express();

// Initialisation typeORM
AppDataSource.initialize()
    .then(() => {
        logger.info("Database connection established successfully");

        // Configuration du serveur Restful
        const server = new WebApiServer({
            port: expressConfig.port,
            route: apiRoutes,
            logger: logger,
            appDataSource: AppDataSource,
            middlewares: [],
            errorHandler: errorHandlerMiddleware
        });

        server.startServer().catch((error) => {
            logger.error("Error starting server:", error.message);
            process.exit(1);
        });
    })
    .catch((error) => {
        logger.error("Error initializing database:", error.message);
        process.exit(1);
    });
