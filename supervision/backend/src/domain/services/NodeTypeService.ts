import { Repository, Like } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { NodesTypes } from "../entities/NodesTypes";

export class NodeTypeService {
    private NodesRepository: Repository<NodesTypes>;
    private nodeTypeRepository: Repository<NodesTypes>;

    constructor() {
        this.NodesRepository = AppDataSource.getRepository(NodesTypes);
        this.nodeTypeRepository = AppDataSource.getRepository(NodesTypes);
    }

    // Méthode pour récupérer les origines
    async getNodeTypes (label: string): Promise<NodesTypes[]> {
        const query = this.NodesRepository.find({
            where: {
                ...(label ? { label: Like(`%${label}%`) } : {}),
            },
        });
        return query;
    }

    async createNodeType(label: string): Promise<NodesTypes> {
        const newNodeType = this.nodeTypeRepository.create({
            label: label
            
        });
        return this.nodeTypeRepository.save(newNodeType);      
    }

    async updateNodeType(id: number, label: string): Promise<NodesTypes> {
        const nodeType = await this.nodeTypeRepository.findOneBy({id: id});
        if (!nodeType) {
            throw new Error("Noeud de voyage non trouvé");
        }
        nodeType.label = label;
        return this.nodeTypeRepository.save(nodeType);
    }
}

