import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateVehicleRequest extends BaseRequest<UpdateVehicleRequest> {
	name : string | null;
	id : number | null;

	constructor(fields?: Partial<UpdateVehicleRequest>) {
		super();
		this.name = fields?.name ?? null;
		this.id = fields?.id ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateVehicleRequest> {
		const validator = new Validator<UpdateVehicleRequest>(this);
		validator.field("name").isRequired();
		validator.field("id").isRequired().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): UpdateVehicleRequest {
		const body = req.body
		const params = req.params;
		return new UpdateVehicleRequest({
			id: params.id,
			name: body.name,
		});
	}
}