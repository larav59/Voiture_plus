import {
  Column,
  Entity,
  Index,
  JoinColumn,
  ManyToOne,
  OneToMany,
  PrimaryGeneratedColumn,
} from "typeorm";
import { Vehicles } from "./Vehicles";
import { TravelsNodes } from "./TravelsNodes";

@Index("vehicle_id", ["vehicleId"], {})
@Entity("TRAVELS", { schema: "supervision" })
export class Travels {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("timestamp", { name: "created_at", nullable: true })
  public createdAt!: Date | null;

  @Column("varchar", { name: "status", nullable: true, length: 255 })
  public status!: string | null;

  @Column("float", {
    name: "estimated_duration",
    nullable: true,
    precision: 12,
  })
  public estimatedDuration!: number | null;

  @Column("int", { name: "vehicle_id", nullable: true })
  public vehicleId!: number | null;

  @Column("varchar", { name: "ordered_by", nullable: true, length: 255 })
  public orderedBy!: string | null;

  @ManyToOne(() => Vehicles, (vehicles) => vehicles.travels, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "vehicle_id", referencedColumnName: "id" }])
  public vehicle!: Vehicles;

  @OneToMany(() => TravelsNodes, (travelsNodes) => travelsNodes.travel)
  public travelsNodes!: TravelsNodes[];

  public constructor(init?: Partial<Travels>) {
    Object.assign(this, init);
  }
}
