import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class UpdateNodeRequest extends BaseRequest<UpdateNodeRequest> {
	id : number;
	name : string | null;
	type : number | null
	positionX : number | null;
	positionY : number | null;
	offsetX : number | null;
	offsetY : number | null;
	isPointOfInterest : boolean | null;

	constructor(fields?: Partial<UpdateNodeRequest>) {
		super();
		this.id = fields?.id ?? 0;
		this.positionX = fields?.positionX ?? null;
		this.positionY = fields?.positionY ?? null;
		this.offsetX = fields?.offsetX ?? null;
		this.offsetY = fields?.offsetY ?? null;
		this.isPointOfInterest = fields?.isPointOfInterest ?? false;
		this.name = fields?.name ?? null;
		this.type = fields?.type ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<UpdateNodeRequest> {
		const validator = new Validator<UpdateNodeRequest>(this);
		validator.field("id").isRequired().isNumber().greaterThan(0);
		validator.field("isPointOfInterest").isBoolean();
		validator.field("positionX").isNumber();
		validator.field("positionY").isNumber();
		validator.field("offsetX").isNumber();
		validator.field("offsetY").isNumber()
		validator.field("name").minLength(3).maxLength(255);
		validator.field("type").isNumber().greaterThan(0);
		return validator;
	}

	static fromRequest(req: any): UpdateNodeRequest {
		const params = req.params
		const body = req.query
		return new UpdateNodeRequest({
			id: params.id,
			positionX : body.positionX,
			positionY : body.positionY,
			offsetX : body.offsetX,
			offsetY : body.offsetY,
			isPointOfInterest: body.isPointOfInterest,
			name: body.name,
			type: body.type
		});
	}
}