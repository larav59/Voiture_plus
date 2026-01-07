import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetNodeRequest extends BaseRequest<GetNodeRequest> {
	isPointOfInterest : boolean | null;

	constructor(fields?: Partial<GetNodeRequest>) {
		super();
		this.isPointOfInterest = fields?.isPointOfInterest ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetNodeRequest> {
		const validator = new Validator<GetNodeRequest>(this);
		validator.field("isPointOfInterest").isBoolean();
		return validator;
	}

	static fromRequest(req: any): GetNodeRequest {
		const query = req.query;
		return new GetNodeRequest({
			isPointOfInterest: query.isPointOfInterest == 'true' ? Boolean(true) : query.isPointOfInterest == 'false' ? Boolean(false) : null,
		});
	}
}