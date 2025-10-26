import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateVehicleRequest extends BaseRequest<UpdateVehicleRequest> {
	name : string | null;

	constructor(fields?: Partial<UpdateVehicleRequest>) {
		super();
		this.name = fields?.name ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateVehicleRequest> {
		const validator = new Validator<UpdateVehicleRequest>(this);
		validator.field("name").isRequired().isString().maxLength(100);
		return validator;
	}

	static fromRequest(req: any): UpdateVehicleRequest {
		const body = req.body
		body.name = body.name !== undefined ? String(body.name) : undefined;

		return new UpdateVehicleRequest({
			name: body.name,
		});
	}
}