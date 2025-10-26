import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateVehiclesRequest extends BaseRequest<CreateVehiclesRequest> {
	name : string | null;

	constructor(fields?: Partial<CreateVehiclesRequest>) {
		super();
		this.name = fields?.name ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateVehiclesRequest> {
		const validator = new Validator<CreateVehiclesRequest>(this);
		validator.field("name").isRequired().isString().maxLength(100);
		return validator;
	}

	static fromRequest(req: any): CreateVehiclesRequest {
		const body = req.body
		body.name = body.name !== undefined ? String(body.name) : undefined;

		return new CreateVehiclesRequest({
			name: body.name,
		});
	}
}