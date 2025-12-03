import { Repository, MoreThan, LessThan } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Arcs } from "../entities/Arcs";
import { Nodes 	} from "../entities/Nodes";

export class ArcService {
	private arcsRepository: Repository<Arcs>;
	private nodesRepository: Repository<Nodes>;

	constructor() {
		this.arcsRepository = AppDataSource.getRepository(Arcs);
		this.nodesRepository = AppDataSource.getRepository(Nodes);
	}

	// Méthode pour récupérer les alarmes
	async getArcs(originNode : number, destinationNode : number): Promise<Arcs[]> {
		const query = this.arcsRepository.find({
			relations : ['originNode2', 'destinationNode2'],
            where: {
                ...(originNode ? {originNodeId : originNode} : {}),
				...(destinationNode ? { destinationNodeId : destinationNode} : {})
            }
        });
        return query;
		
	}

	async createArc(weight: number, originNodeId: number, destinationNodeId : number): Promise<Arcs> {
		const originNode = this.nodesRepository.findOneBy({id: originNodeId});
		if (originNode == null ) throw new Error("Noeud d'origine non trouvé");
		const destinationNode = this.nodesRepository.findOneBy({id: destinationNodeId});
		if (destinationNode == null ) throw new Error("Noeud de destination non trouvé");
		
		const newArc = this.arcsRepository.create({
			weight : weight,
			originNodeId : originNodeId,
			destinationNodeId : destinationNodeId
		});
		return newArc ;
	}

	async updateArc(id: number, weight: number, originNodeId: number, destinationNodeId : number): Promise<Arcs> {
		const arc = await this.arcsRepository.findOneBy({ id : id });
		if (arc == null) throw new Error("Arc spécifié non trouvé");
		arc.originNodeId = originNodeId;
		arc.destinationNodeId = destinationNodeId;
		arc.weight = weight;
		return this.arcsRepository.save(arc);
	}
}

