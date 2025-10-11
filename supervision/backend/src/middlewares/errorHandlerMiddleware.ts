import { Request, Response, NextFunction } from "express";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { HttpError } from "../infrastructure/webserver/errors/HttpError";
import { logger } from "../infrastructure/loggers";

export function errorHandlerMiddleware(err: any, req: Request, res: Response, next: NextFunction) {
    if (err instanceof HttpError) {
        return res.status(err.statusCode).json({
			message: err.message,
			data: err.data || null
		})
    }

	logger.error("[ErrorHandler]", err);
    return res.status(HttpStatusEnum.INTERNAL_SERVER_ERROR).json({
        message: "Internal server error",
        data: null
    });
}
