import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateOriginRequest extends BaseRequest<UpdateOriginRequest> {
	label : string;
	id : number ;

	constructor(fields?: Partial<UpdateOriginRequest>) {
		super();
		this.label = fields?.label !;
		this.id = fields?.id !;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateOriginRequest> {
		const validator = new Validator<UpdateOriginRequest>(this);
		validator.field("label").maxLength(255);
		validator.field("id").isRequired().isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): UpdateOriginRequest {
		const body = req.body
		const params = req.params;
		return new UpdateOriginRequest({
			id: Number(params.id),
			label: body.label,
		});
	}
}