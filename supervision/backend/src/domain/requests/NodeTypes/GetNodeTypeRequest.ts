import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class GetNodeTypeRequest extends BaseRequest<GetNodeTypeRequest> {
	label : string | null;

	constructor(fields?: Partial<GetNodeTypeRequest>) {
		super();
		this.label = fields?.label ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<GetNodeTypeRequest> {
		const validator = new Validator<GetNodeTypeRequest>(this);
		validator.field("label").minLength(3).maxLength(255);
		return validator;
	}

	static fromRequest(req: any): GetNodeTypeRequest {
		const query = req.query;
		return new GetNodeTypeRequest({
			label: query.label,
		});
	}
}