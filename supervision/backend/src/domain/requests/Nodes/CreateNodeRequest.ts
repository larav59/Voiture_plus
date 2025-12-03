import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class CreateNodeRequest extends BaseRequest<CreateNodeRequest> {
	name : string | null;
	type : number | null
	positionX : number | null;
	positionY : number | null;
	offsetX : number | null;
	offsetY : number | null;
	isPointOfInterest : boolean | null;

	constructor(fields?: Partial<CreateNodeRequest>) {
		super();
		this.name = fields?.name ?? null;
		this.type = fields?.type ?? null;
		this.positionX = fields?.positionX ?? null;
		this.positionY = fields?.positionY ?? null;
		this.offsetX = fields?.offsetX ?? null;
		this.offsetY = fields?.offsetY ?? null;
		this.isPointOfInterest = fields?.isPointOfInterest ?? null;
	}

	// Méthode pour la validation
	public validate(): Validator<CreateNodeRequest> {
		const validator = new Validator<CreateNodeRequest>(this);
		validator.field("type").isNumber().greaterThan(0);
		validator.field("positionX").isRequired().isNumber();
		validator.field("positionY").isRequired().isNumber();
		validator.field("offsetX").isRequired().isNumber();
		validator.field("offsetY").isRequired().isNumber()
		validator.field("name").isRequired().minLength(3).maxLength(255);
		validator.field("isPointOfInterest").isRequired().isBoolean();
		return validator;
	}

	static fromRequest(req: any): CreateNodeRequest {
		const body = req.body;
		return new CreateNodeRequest({
			name: body.name,
			type: body.type,
			positionX : body.position.positionX,
			positionY : body.position.positionY,
			offsetX : body.position.offsetX,
			offsetY : body.position.offsetY,
			isPointOfInterest: body.isPointOfInterest,
		});
	}
}