import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateNodeTypeRequest extends BaseRequest<UpdateNodeTypeRequest> {
	id : number;
	label : string | null;

	constructor(fields?: Partial<UpdateNodeTypeRequest>) {
		super();
		this.id = fields?.id ?? 0;
		this.label = fields?.label ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateNodeTypeRequest> {
		const validator = new Validator<UpdateNodeTypeRequest>(this);
		validator.field("id").isRequired().isNumber().greaterThan(0);
		validator.field("label").isRequired().minLength(3).maxLength(255);
		return validator;
	}

	static fromRequest(req: any): UpdateNodeTypeRequest {
		const params = req.params;
		const body = req.body;
		return new UpdateNodeTypeRequest({
			id: params.id,
			label: body.label
		});
	}
}