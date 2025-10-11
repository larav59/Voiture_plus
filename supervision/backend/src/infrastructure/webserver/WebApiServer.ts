import express, { Application, Request, Response, NextFunction, IRoute, Router } from "express";
import bodyParser from "body-parser";
import compression from "compression";
import cors from "cors";
import { Logger } from "../loggers/Logger";
import { NotFound } from "./errors/HttpError";
import { DataSource } from "typeorm";

export interface WebApiServerOptions {
    port: number;
    route: Router;              // route principale (ex: /api/)
    middlewares?: any[];        // middlewares globaux supplémentaires
    errorHandler?: any;         // middleware de gestion des erreurs (par défaut errorHandlerMiddleware)
    logger: Logger;
    appDataSource?: DataSource; // optionnel si pas de DB
}

export class WebApiServer {
    private app: Application;
    private port: number;
    private route: Router;
    private errorHandler?: any;
    private middlewares: any[];
    private logger: Logger;
    private appDataSource?: DataSource;

    constructor(options: WebApiServerOptions) {
        this.app = express();
        this.port = options.port;
        this.route = options.route
        this.middlewares = options.middlewares || [];
        this.logger = options.logger;
        this.appDataSource = options.appDataSource;
        this.errorHandler = options.errorHandler;
    }

    async startServer() {
        this.initialiseServer();

        this.app.listen(this.port, () => {
            this.logger.info(`Server is running on http://localhost:${this.port}`);
        });

        ["SIGINT", "SIGTERM"].forEach((signal) => {
            process.on(signal, () => this.handleSignal());
        });
    }

    private initialiseServer() {
        // Middlewares génériques
        this.app.use(bodyParser.json());
        this.app.use(compression());
        this.app.use(express.urlencoded({ extended: true }));
        this.app.use(cors());

        // Middlewares globaux injectables
        this.middlewares.forEach((mw) => this.app.use(mw));

        this.app.use("/api", this.route);

        this.app.use("/*", (req: Request, res: Response, next: NextFunction) => {
            next(new NotFound(`Resource ${req.path} not found`));
        });

        // Middleware d'erreurs global
        if(!this.errorHandler) {
            this.app.use(this.errorHandler);
        }
    }

    private async handleSignal() {
        try {
            if (this.appDataSource) {
                this.logger.info("Closing database connection...");
                await this.appDataSource.destroy();
                this.logger.info("Database connection closed");
            }
            this.logger.info("Server shutdown complete");
            process.exit(0);
        } catch (error: any) {
            this.logger.error("Error during shutdown:", error.message);
            process.exit(1);
        }
    }
}
