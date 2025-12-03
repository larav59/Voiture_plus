import {
  Column,
  Entity,
  Index,
  JoinColumn,
  ManyToOne,
  PrimaryGeneratedColumn,
} from "typeorm";
import { Travels } from "./Travels";
import { Nodes } from "./Nodes";

@Index("node_id", ["nodeId"], {})
@Index("travel_id", ["travelId"], {})
@Entity("TRAVELS_NODES", { schema: "supervision" })
export class TravelsNodes {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("int", { name: "travel_id", nullable: true })
  public travelId!: number | null;

  @Column("int", { name: "node_id", nullable: true })
  public nodeId!: number | null;

  @Column("int", { name: "order", nullable: true })
  public order!: number | null;

  @Column("timestamp", { name: "arrived_at", nullable: true })
  public arrivedAt!: Date | null;

  @ManyToOne(() => Travels, (travels) => travels.travelsNodes, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "travel_id", referencedColumnName: "id" }])
  public travel!: Travels;

  @ManyToOne(() => Nodes, (nodes) => nodes.travelsNodes, {
    onDelete: "NO ACTION",
    onUpdate: "NO ACTION",
  })
  @JoinColumn([{ name: "node_id", referencedColumnName: "id" }])
  public node!: Nodes;

  public constructor(init?: Partial<TravelsNodes>) {
    Object.assign(this, init);
  }
}
