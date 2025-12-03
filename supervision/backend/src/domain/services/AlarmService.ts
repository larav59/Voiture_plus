import { Repository, MoreThan, LessThan } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Alarms } from "../entities/Alarms";
import { AlarmsTypes } from "../entities/AlarmsTypes";
import { Origins } from "../entities/Origins";

export class AlarmService {
	private alarmRepository: Repository<Alarms>;
	private alarmTypeRepository: Repository<AlarmsTypes>;
	private originRepository: Repository<Origins>;

	constructor() {
		this.alarmRepository = AppDataSource.getRepository(Alarms);
		this.alarmTypeRepository = AppDataSource.getRepository(AlarmsTypes);
		this.originRepository = AppDataSource.getRepository(Origins);
	}

	// Méthode pour récupérer les alarmes
	async getAlarms(originId: number | null, occuredBefore : Date | null, occuredAfter: Date | null, typeId: number | null): Promise<Alarms[]> {
		const query = this.alarmRepository.find({
			relations : ['alarmType', 'origin'],
			where: {
				...(originId !== null ? { originId: originId } : {}),
				...(occuredBefore !== null ? { occuredAt: LessThan(occuredBefore) } : {}),
				...(occuredAfter !== null ? { occuredAt: MoreThan(occuredAfter) } : {}),
				...(typeId !== null ? { typeId: typeId } : {})
			}
		});
		return query;
	}

	async createAlarm(description: string, typeId: number, originId: number): Promise<Alarms> {
		const type = await this.alarmTypeRepository.findOneBy({ id: typeId });
		const origin = await this.originRepository.findOneBy({ id: originId });
		const newAlarm = this.alarmRepository.create({
			description: description,
			alarmTypeId: type?.id ?? null,
			originId: origin?.id ?? null,
			createdAt: new Date()
		});
		return this.alarmRepository.save(newAlarm);
	}

	async updateAlarm(id: number, description: string, typeId: number, originId: number): Promise<Alarms | null> {
		const alarm = await this.alarmRepository.findOneBy({ id });
		const type = await this.alarmTypeRepository.findOneBy({ id: typeId });
		const origin = await this.originRepository.findOneBy({ id: originId });
		if (!alarm) {
			return null;
		}
		alarm.description = description;
		alarm.alarmType = type?.id ? type : alarm.alarmType;
		alarm.originId = origin?.id ? origin.id : alarm.originId;
		return this.alarmRepository.save(alarm);
	}
}

