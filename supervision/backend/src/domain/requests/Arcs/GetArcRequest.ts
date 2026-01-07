import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetArcRequest extends BaseRequest<GetArcRequest> {
	originNode !: number | null;
	destinationNode !: number | null;
	type !: number | null;

	constructor(fields?: Partial<GetArcRequest>) {
		super();
		this.originNode = fields?.originNode ?? null;
		this.destinationNode = fields?.destinationNode ?? null;
		this.type = fields?.type ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetArcRequest> {
		const validator = new Validator<GetArcRequest>(this);
		validator.field("originNode").isNumber().greaterThan(0);
		validator.field("destinationNode").isNumber().greaterThan(0);
		validator.field("type").isRequired().isNumber();
		return validator;
	}

	static fromRequest(req: any): GetArcRequest {
		const query = req.query;
		return new GetArcRequest({
			originNode: query.originNode ? Number(query.originNode) : null,
			destinationNode: query.destinationNode ? Number(query.destinationNode) : null,
			type: query.type ? Number(query.type) : null
		});
	}
}