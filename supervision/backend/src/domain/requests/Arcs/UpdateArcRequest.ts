import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateArcRequest extends BaseRequest<UpdateArcRequest> {
	id : number;
	weight : number;
	originNode : number;
	destinationNode : number;
	type : number | null;

	constructor(fields?: Partial<UpdateArcRequest>) {
		super();
		this.id = fields?.id !;
		this.weight = fields?.weight !;
		this.originNode = fields?.originNode !;
		this.destinationNode = fields?.destinationNode !;
		this.type = fields?.type ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateArcRequest> {
		const validator = new Validator<UpdateArcRequest>(this);
		validator.field("id").isRequired().isNumber().greaterThan(0);
		validator.field("originNode").isNumber().greaterThan(0);
		validator.field("destinationNode").isNumber().greaterThan(0);
		validator.field("weight").isRequired().isNumber().greaterThan(0);
		validator.field("type").isRequired().isNumber();
		return validator;
	}

	static fromRequest(req: any): UpdateArcRequest {
		const params = req.params;
		const body = req.body;
		return new UpdateArcRequest({
			id: Number(params.id),
			weight: body.weight,
			originNode: Number(body.originNode),
			destinationNode: Number(body.destinationNode),
			type: Number(body.type)
		});
	}
}