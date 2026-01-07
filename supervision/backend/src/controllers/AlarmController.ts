import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
	GetAlarmRequest,
	CreateAlarmRequest,
	UpdateAlarmRequest
} from "../domain/requests/Alarms";
import { AlarmService } from "../domain/services/AlarmService";
import { AlarmsDTO } from "../domain/dtos/AlarmsDTO";

export class AlarmController {
	
	async ping(req: Request, res: Response) {
		res.status(HttpStatusEnum.OK).json({ message: "pong" });
	}

	@Authenticated()
	public async getAlarms(req: Request, res: Response) : Promise<void> {
		const request = GetAlarmRequest.fromRequest(req);
		const alarmsService = new AlarmService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}
		const alarms = await alarmsService.getAlarms(request.originId, request.occuredAfter, request.occuredBefore, request.typeId);
		const alarmsDTO = alarms.map(alarm => AlarmsDTO.fromEntity(alarm));
		res.status(HttpStatusEnum.OK).json(alarmsDTO);
		return;
	}

	@Authenticated()
	public async createAlarm(req: Request, res: Response): Promise<void> {
		const request = CreateAlarmRequest.fromRequest(req);
		const alarmService = new AlarmService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const alarm = await alarmService.createAlarm(request.description ?? "", request.type!, request.origin!);
		const alarmDTO = AlarmsDTO.fromEntity(alarm);
		res.status(HttpStatusEnum.OK).json(alarmDTO);
		return;
	}

	@Authenticated()
	public async updateAlarm(req: Request, res: Response): Promise<void> {
		const request = UpdateAlarmRequest.fromRequest(req);
		const alarmService = new AlarmService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const alarm = await alarmService.updateAlarm(request.id, request.description, request.type, request.origin);
		if (!alarm) {
			throw res.status(HttpStatusEnum.NOT_FOUND).json({ message: "Alarm not found" });
		}
		const alarmDTO = AlarmsDTO.fromEntity(alarm);
		res.status(HttpStatusEnum.OK).json(alarmDTO);
		return ;
	}

	@Authenticated()
	async echo(req: Request, res: Response) {
		res.json({
			identity: req.identity,
			body: req.body
		});
	}

	async status(req: Request, res: Response) {
		throw new NotImplemented("Status endpoint not implemented yet");
	}
}

