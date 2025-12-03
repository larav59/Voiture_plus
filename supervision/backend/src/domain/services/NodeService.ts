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
            relations:['nodeType2'],
            where: {
                ...(isPointOfInterest ? { pointOfInterest: isPointOfInterest } : {}),
            },
        });
        return query;
    }

    async createNode(name: string, position: any, isPointOfInterest: boolean , type: number): Promise<Nodes> {
        const nodeType = await this.nodeTypeRepository.findOneBy({id: type});
        if (!nodeType) {
            throw new Error("Type de noeud non trouvé");
        }
        if (position.x === undefined || position.y === undefined || position.offset_x === undefined || position.offset_y === undefined) {
            throw new Error("Position invalide");
        }
        const newTravelNode = this.NodesRepository.create({
            name: name,
            positionX: position.x,
            positionY: position.y,
            pointOfInterest: isPointOfInterest,
            offsetX: position.offsetX,
            offsetY: position.offsetY,
            nodeTypeId: type
        });
        return this.NodesRepository.save(newTravelNode);
        
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

