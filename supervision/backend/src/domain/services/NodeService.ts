import { Repository, Like, LessThanOrEqual, MoreThanOrEqual } from "typeorm";
import { AppDataSource } from "../../infrastructure/database/AppDataSource";
import { Nodes } from "../entities/Nodes";
import { NodesTypes } from "../entities/NodesTypes";

export class NodeService {
    private NodesRepository: Repository<Nodes>;
    private nodeTypeRepository: Repository<NodesTypes>;

    constructor() {
        this.NodesRepository = AppDataSource.getRepository(Nodes);
        this.nodeTypeRepository = AppDataSource.getRepository(NodesTypes);
    }

    // Méthode pour récupérer les origines
    async getNodes(isPointOfInterest : boolean | false): Promise<Nodes[]> {
        const query = this.NodesRepository.find({
            relations:['nodeType'],
            where: {
                ...({ pointOfInterest: isPointOfInterest }),
            },
        });
        return query;
    }

    async createNode(name: string, positionX: number, positionY: number, offsetX: number, offsetY: number, isPointOfInterest: boolean , type: number): Promise<Nodes> {
        const nodeType = await this.nodeTypeRepository.findOneBy({id: type});
        if (!nodeType) {
            throw new Error("Type de noeud non trouvé");
        }
        if (positionX === undefined || positionY === undefined || offsetX === undefined || offsetY === undefined) {
            throw new Error("Position invalide");
        }
        const newTravelNode = this.NodesRepository.create({
            name: name,
            positionX: positionX,
            positionY: positionY,
            pointOfInterest: isPointOfInterest,
            offsetX: offsetX,
            offsetY: offsetY,
            nodeTypeId: nodeType.id
        });
        await this.NodesRepository.save(newTravelNode);
        // Récupérer le noeud avec les relations après la sauvegarde
        const savedNode = await this.NodesRepository.findOne({
            where: { id: newTravelNode.id },
            relations: ['nodeType'],
        });
        if (!savedNode) {
            throw new Error("Erreur lors de la récupération du noeud après la création");
        }
        return savedNode;
    }

    async updateNode(id: number, name: string, position: any, isPointOfInterest: boolean, type: number): Promise<Nodes> {
        const node = await this.NodesRepository.findOneBy({id: id});
        if (!node) {
            throw new Error("Noeud de voyage non trouvé");
        }
        if (name !== undefined) node.name = name;
        if (position.x !== undefined)  node.positionX = position.x;
        if (position.y !== undefined)  node.positionY = position.y;
        if (position.offsetX !== undefined)  node.offsetX = position.offsetX;
        if (position.offsetY !== undefined)  node.offsetY = position.offsetY;
        if (isPointOfInterest !== undefined) node.pointOfInterest = isPointOfInterest;
        if (type !== undefined) node.nodeTypeId = type;

        return this.NodesRepository.save(node);
        
    }
}

