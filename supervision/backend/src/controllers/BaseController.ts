import { Response } from "express";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { Paginator } from "../infrastructure/pagination";

/**
 * BaseController : classe de base pour les contrôleurs, avec des méthodes utilitaire
 */
export abstract class BaseController {

	public sendPaginatedResponse<T>(res: Response, data: T[], pageSize: number, pageNo: number): void {
		const paginatedData = new Paginator<T>(data, pageSize, pageNo).getPaginatedData();
		res.status(HttpStatusEnum.OK).json(paginatedData);
	}
}