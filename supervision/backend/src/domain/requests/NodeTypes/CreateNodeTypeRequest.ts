import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateNodeTypeRequest extends BaseRequest<CreateNodeTypeRequest> {
	label : string
	
	constructor(fields?: Partial<CreateNodeTypeRequest>) {
		super();
		this.label = fields?.label !;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateNodeTypeRequest> {
		const validator = new Validator<CreateNodeTypeRequest>(this);
		validator.field("label").isRequired().minLength(3).maxLength(255);
		return validator;
	}

	static fromRequest(req: any): CreateNodeTypeRequest {
		const body = req.body
		return new CreateNodeTypeRequest({
			label: body.label,
		});
	}
}