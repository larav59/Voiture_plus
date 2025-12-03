import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateArcRequest extends BaseRequest<CreateArcRequest> {
	weight : number | null;
	origineId : number | null;
	destinationId : number | null;

	constructor(fields?: Partial<CreateArcRequest>) {
		super();
		this.weight = fields?.weight ?? null;
		this.origineId = fields?.origineId ?? null;
		this.destinationId = fields?.destinationId ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateArcRequest> {
		const validator = new Validator<CreateArcRequest>(this);
		validator.field("weight").isNumber().greaterThan(0);
		validator.field("origineId").isRequired().isNumber().greaterThan(0);
		validator.field("destinationId").isRequired().isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): CreateArcRequest {
		return new CreateArcRequest({
			weight: req.body.description,
			destinationId: req.body.destinationId,
			origineId: req.body.origineId,
		});
	}
}