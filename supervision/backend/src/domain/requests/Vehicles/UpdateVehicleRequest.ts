import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateVehicleRequest extends BaseRequest<UpdateVehicleRequest> {
	name : string | "";
	id : number | 0;

	constructor(fields?: Partial<UpdateVehicleRequest>) {
		super();
		this.name = fields?.name ?? "";
		this.id = fields?.id ?? 0;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateVehicleRequest> {
		const validator = new Validator<UpdateVehicleRequest>(this);
		validator.field("name").isRequired();
		validator.field("id").isRequired();
		return validator;
	}

	static fromRequest(req: any): UpdateVehicleRequest {
		const body = req.body
		const params = req.params;
		params.id = params.id !== undefined ? parseInt(params.id) : undefined;
		body.name = body.name !== undefined ? String(body.name) : undefined;

		return new UpdateVehicleRequest({
			id: params.id,
			name: body.name,
		});
	}
}