import { Repository, MoreThan, LessThan } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Users } from "../entities/Users";

export class LoginService {
	private userRepository: Repository<Users>;

	constructor() {
		this.userRepository = AppDataSource.getRepository(Users);
	}

	// Méthode pour récupérer les alarmes
	async login(username: string, password: string): Promise<Boolean> {
		const query = this.userRepository.find({
			where: {
				...(username ? { username: username } : {}),
				...(password ? { password: password } : {}),
			}
		});
		if ((await query).length > 0) return true;
		return false;
	}
}

