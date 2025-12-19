import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateArcRequest extends BaseRequest<CreateArcRequest> {
	weight : number ;
	originNode : number ;
	destinationNode : number ;
	type : number | null;

	constructor(fields?: Partial<CreateArcRequest>) {
		super();
		this.weight = fields?.weight!;
		this.originNode = fields?.originNode!;
		this.destinationNode = fields?.destinationNode!;
		this.type = fields?.type ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateArcRequest> {
		const validator = new Validator<CreateArcRequest>(this);
		validator.field("weight").isRequired().isNumber().greaterThan(0);
		validator.field("originNode").isRequired().isNumber().greaterThan(0);
		validator.field("destinationNode").isRequired().isNumber().greaterThan(0);
		validator.field("type").isRequired().isNumber();
		return validator;
	}

	static fromRequest(req: any): CreateArcRequest {
		const body = req.body;
		return new CreateArcRequest({
			weight: Number(body.weight),
			destinationNode: Number(body.destinationNode),
			originNode: Number(body.originNode),
			type: Number(body.type)
		});
	}
}