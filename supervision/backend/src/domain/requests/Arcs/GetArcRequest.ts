import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetArcRequest extends BaseRequest<GetArcRequest> {
	origineId : number | null;
	destinationId : number | null;

	constructor(fields?: Partial<GetArcRequest>) {
		super();
		this.origineId = fields?.origineId ?? null;
		this.destinationId = fields?.destinationId ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetArcRequest> {
		const validator = new Validator<GetArcRequest>(this);
		validator.field("origineId").isNumber().greaterThan(0);
		validator.field("destinationId").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): GetArcRequest {
		return new GetArcRequest({
			origineId: req.query.origineId,
			destinationId: req.query.destinationId,
		});
	}
}