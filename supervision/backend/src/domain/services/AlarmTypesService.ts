import { Repository, Like } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { AlarmsTypes } from "../entities/AlarmsTypes";
export class AlarmTypesService {
	private alarmsTypesRepository: Repository<AlarmsTypes>;

	constructor() {
		this.alarmsTypesRepository = AppDataSource.getRepository(AlarmsTypes);
	}

	// Méthode pour récupérer les alarmes
	async getAlarmTypes(label: string | null, criticity: string | null): Promise<AlarmsTypes[]> {
		const query = this.alarmsTypesRepository.find({
			where: {
				...( label ? { label : Like(`%${label}%`) } : {}),
				...(criticity ? { criticity : Like(`%${criticity}%`) } : {})
			}
		});
		return query;
	}

	async createAlarmTypes(label: string,criticity: string): Promise<AlarmsTypes> {
		const newAlarm = this.alarmsTypesRepository.create({
			label: label,
			criticity: criticity
		});
		return this.alarmsTypesRepository.save(newAlarm);
	}

	async updateAlarmTypes(id: number, label: string | null,criticity: string | null): Promise<AlarmsTypes | null> {
		const alarmType = await this.alarmsTypesRepository.findOneBy({ id });
		if (!alarmType) {
			return null;
		}
		if (label !== null) {
			alarmType.label = label;
		}
		if (criticity !== null) {
			alarmType.criticity = criticity;
		}
		return this.alarmsTypesRepository.save(alarmType);
	}
}

